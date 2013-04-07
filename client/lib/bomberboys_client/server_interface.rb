module BomberboysClient
  class ServerInterface 
    attr_reader :packet_loss

    def initialize(socket)
      @socket = socket
      @client_uid_count = 0
      @server_uid_count = -1
      @packet_loss = 0

      @receive_mutex = Mutex.new
      @send_mutex = Mutex.new
    end

    def receive
      @receive_mutex.synchronize do
        begin
          message = Message.unpack(@socket.recv(150))
        end while message.uid < @server_uid_count

        @packet_loss += message.uid - @server_uid_count - 1
        @server_uid_count = message.uid

        message
      end
    end

    def send(message)
      @send_mutex.synchronize do
        message.uid = @client_uid_count
        @socket.print(message.pack)

        @client_uid_count += 1
      end
    end
  end
end
