module BomberboysClient
  class Connection 
    def initialize(socket)
      @socket = socket
    end

    def register(listener)
      @listener = listener 
    end

    def start
      while str_message = @socket.gets
        message = Message.unpack(str_message)
        listener.send(message.action, *message.params)
      end
    end

    def login(name)
      @socket.print Message.new(:login, [name]).pack
    end

    def move_me(direction)
      @socket.print Message.new(:move_me, [direction]).pack
    end

    def plant_bomb
      @socket.print Message.new(:plant_bomb).pack
    end
  end
end
