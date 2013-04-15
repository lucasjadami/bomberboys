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
      @processed_messages = []
    end

    def receive
      while @processed_messages.empty?
        @buffer << @socket.recv(256)
        process_buffer
      end

      begin
        message = @processed_messages.shift
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

    def close
      @socket.close
    end

    private
    def process_buffer
      uid, action = @buffer.slice(0..4).unpack('NC')
      while is_ready_to_process(action)
        uid, action = @buffer.slice!(0..4).unpack('NC')
        str_params  = @buffer.slice!(0...Message::BODY_SIZE[action])
        @buffer.slice!(0...@trash_size)

        @processed_messages << Message.new(action, str_params, uid)

        uid, action = @buffer.slice(0..4).unpack('NC')
      end
    end

    def is_ready_to_process(action)
      Message::BODY_SIZE[action] && @buffer.size - 5 >= Message::BODY_SIZE[action] + @trash_size
    end

    def append_trash(str)
      str << '*' * @trash_size
    end
  end
end
