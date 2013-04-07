module BomberboysClient
  class ServerInterface 
    attr_reader :packet_loss

    def initialize(socket, trash_size = 0)
      @socket = socket
      @client_uid_count = 0
      @server_uid_count = -1
      @packet_loss = 0
      @trash_size  = trash_size

      @receive_mutex = Mutex.new
      @send_mutex = Mutex.new
    end

    def receive
      @receive_mutex.synchronize do
        begin
          uid, action = @socket.recv(5).unpack('NC')
          str_params  = @socket.recv(Message::BODY_SIZE[action])
          @socket.recv(@trash_size)

          message = Message.new(action, str_params, uid)
        end while message.uid < @server_uid_count

        @packet_loss += message.uid - @server_uid_count - 1
        @server_uid_count = message.uid

        message
      end
    end

    def send(message)
      @send_mutex.synchronize do
        message.uid = @client_uid_count

        @socket.print(append_trash(message.pack))

        @client_uid_count += 1
      end
    end

    private
    def append_trash(str)
      str << '*' * @trash_size
    end
  end
end
