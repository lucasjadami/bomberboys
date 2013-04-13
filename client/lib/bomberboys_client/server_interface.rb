module BomberboysClient
  class ServerInterface 
    attr_reader :packet_loss

    def initialize(socket, trash_size = 0)
      @socket = socket
      @client_uid_count = 0
      @server_uid_count = -1
      @packet_loss = 0
      @trash_size  = trash_size
      @buffer      = ""
    end

    def receive
      begin
        begin 
          @buffer << @socket.recv(100 + @trash_size)
          uid, action = @buffer.slice(0..4).unpack('NC')
        end until Message::BODY_SIZE[action] && @buffer.size - 5 >= Message::BODY_SIZE[action] + @trash_size

        uid, action = @buffer.slice!(0..4).unpack('NC')
        str_params  = @buffer.slice!(0...Message::BODY_SIZE[action])
        @buffer.slice!(0...@trash_size)

        message = Message.new(action, str_params, uid)
      end while message.uid < @server_uid_count

      @packet_loss += message.uid - @server_uid_count - 1
      @server_uid_count = message.uid

      message
    end

    def send(message)
      message.uid = @client_uid_count

      @socket.print(append_trash(message.pack))

      @client_uid_count += 1
    end

    private
    def append_trash(str)
      str << '*' * @trash_size
    end
  end
end
