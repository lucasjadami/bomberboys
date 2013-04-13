module BomberboysClient
  class Client
    def initialize(player)
      @world = World.new
      @player = player
      @server = @player.socket
    end

    def connect
      login
    end

    def start
      @thread = Thread.new do
        begin
          while message = @server.receive
            modify_world(message)
            @player.react(@world)
          end
        rescue Exception => ex
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

    private
    def login
      @server.send(Message.new(:login, [@player.name]))
    end
  end
end
