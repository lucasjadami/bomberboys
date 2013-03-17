module BomberboysClient
  class Connection 
    def initialize(address, port)
    end

    def register(listener)
      @listener = listener 
    end

    def start
      str_message = "h\n\n\n\n\n\nasdf"
      loop do
        message = Message.unpack(str_message)
        listener.send(message.action, *message.params)
      end
    end

    def login(name)
      Message.new(:login, [name]).pack
    end

    def move_me(direction)
      Message.new(:move_me, [direction]).pack
    end

    def plant_bomb
      Message.new(:plant_bomb).pack
    end
  end
end
