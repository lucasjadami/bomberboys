module BomberboysClient
  class Client
    def initialize(server, login_name)
      @server = server 
      @login_name = login_name
      @world = World.new
    end

    def connect
      login
      modify_world(@server.receive)
    end

    def start
      Thread.new do
        while message = @server.receive
          modify_world(message)
        end
      end
    end

    def modify_world(message)
      @world.send(message.action, *message.params) if @world.respond_to?(message.action)
    end

    def ack
      @server.send(Message.new(:ack))
    end

    private
    def login
      @server.send(Message.new(:login, [@login_name]))
    end
  end
end
