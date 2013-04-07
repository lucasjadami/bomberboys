module BomberboysClient
  class ServerInterface 
    def initialize(socket)
      @socket = socket
    end

    def receive
      Message.unpack(@socket.recv(150))
    end

    def send(message)
      @socket.print(message.pack)
    end
  end
end
