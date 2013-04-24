require 'thread'

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

      @send_queue = Queue.new
      @receive_queue = Queue.new
    end

    def start
      start_sender
      start_receiver
    end

    def receive
      begin
        message = @receive_queue.pop
      end while message.uid < @server_uid_count

      @packet_loss += message.uid - @server_uid_count - 1
      @server_uid_count = message.uid

      message
    end

    def send(message)
      @send_queue << message
    end

    def close
      @receive_thread.kill
      @send_thread.kill
      @receive_queue.clear
      @send_queue.clear
      @socket.shutdown(:SHUT_RDWR)
      @socket.close
    end

    private
    def start_sender
      @send_thread = Thread.new do
        loop do
          message = @send_queue.pop
          message.uid = @client_uid_count
          @socket.print(append_trash(message.pack))
          @client_uid_count += 1
        end
      end
    end

    def start_receiver
      @receive_thread = Thread.new do
        loop do
          @buffer << @socket.recv(256)

          uid, action = @buffer.slice(0..4).unpack('NC')
          while is_ready_to_process(action)
            uid, action = @buffer.slice!(0..4).unpack('NC')
            str_params  = @buffer.slice!(0...Message::BODY_SIZE[action])
            @buffer.slice!(0...@trash_size)

            @receive_queue << Message.new(action, str_params, uid)

            uid, action = @buffer.slice(0..4).unpack('NC')
          end
        end
      end
    end

    def is_ready_to_process(action)
      Message::BODY_SIZE[action] && @buffer.size - 5 >= Message::BODY_SIZE[action] + @trash_size
    end

    def append_trash(str)
      @trash ||= '*' * @trash_size
      str << @trash
    end
  end
end
