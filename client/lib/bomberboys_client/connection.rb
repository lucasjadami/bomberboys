module BomberboysClient
  class Connection 
    def initialize(socket)
      @socket = socket
    end

    def register(listener)
      @listener = listener 
    end

    def start
      while str_message = @socket.recv(100)
        message = Message.unpack(str_message)
        if @listener.respond_to?(message.action)
          @listener.send(message.action, *message.params)
        else
          puts "ERROR: Listener doesn't respond to #{message.action}."
        end
      end
    end

    def login(name)
      @socket.print Message.new(:login, [name]).pack
    end

    def move_me(direction)
      dir = { n:0, ne:1, e:2, se:3, s:4, sw:5, w:6, nw:7 }
      @socket.print Message.new(:move_me, [dir[direction]]).pack
    end

    def plant_bomb
      @socket.print Message.new(:plant_bomb).pack
    end
  end
end
