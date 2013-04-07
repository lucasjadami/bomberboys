module BomberboysClient
  class Client
    def initialize(server, login)
      @server = server 
      @login_name = login
    end

    def register(listener)
      @listener = listener 
    end

    def connect
      login
      send_to_listeners(@server.receive)
    end

    def start
      Thread.new do 
        loop { sleep 5; ack }
      end

      Thread.new do
        while message = @server.receive
          send_to_listeners(message)
        end
      end
    end

    def send_to_listeners(message)
      @listener.send(message.action, *message.params) if @listener.respond_to?(message.action)
    end

    def move_me(direction)
      dir = { n:0, ne:1, e:2, se:3, s:4, sw:5, w:6, nw:7 }
      @server.send(Message.new(:move_me, [dir[direction]]))
    end

    def plant_bomb
      @server.send(Message.new(:plant_bomb))
    end

    private
    def login
      @server.send(Message.new(:login, [@login_name]))
    end

    def ack
      @server.send(Message.new(:ack))
      puts 'ack!'
    end
  end
end
