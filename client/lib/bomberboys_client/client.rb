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
        while message = @server.receive
          send_to_listeners(message)
        end
      end
    end

    def send_to_listeners(message)
      @listener.send(message.action, *message.params) if @listener.respond_to?(message.action)
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
