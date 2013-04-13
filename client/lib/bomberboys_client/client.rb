module BomberboysClient
  class Client
    def initialize(player)
      @world = World.new
      @player = player
      @server = @player.socket
      @times  = []
      @shutdown = false
    end

    def connect
      login
    end

    def start
      shutdown = false

      @thread = Thread.new do
        begin
          while (message = @server.receive) && !@shutdown
            case message.action
              when :pong
                @times << (Time.now - @start_time)
              when :shutdown
                send_info
                @shutdown = true
                puts "Shutting down client #{@player.name}. (msg received)"
              else
                modify_world(message)
                @player.react(@world)
            end
          end
        rescue
          puts "Shutting down client #{@player.name}."
        end
      end
    end

    def join
      @thread.join
    end

    def modify_world(message)
      @world.send(message.action, *message.params) if @world.respond_to?(message.action)
    end

    def ack
      @server.send(Message.new(:ack))
    end

    def ping
      @start_time = Time.now
      @server.send(Message.new(:ping)) unless @shutdown
    end

    private
    def login
      @server.send(Message.new(:login, [@player.name]))
    end

    def send_info
      m = time_mean.to_r
      d = time_dev.to_r
      l = @server.packet_loss

      @server.send(Message.new(:info, [m.numerator, m.denominator, d.numerator, d.denominator, l]))
    end

    def time_mean
      @times.inject(0) { |accum, i| accum + i } / @times.length.to_f
    end

    def time_dev
      Math.sqrt(time_variance)
    end

    def time_variance
      m = time_mean
      @times.inject(0) { |accum, i| accum + (i - m)**2 } / (@times.length - 1).to_f
    end
  end
end
