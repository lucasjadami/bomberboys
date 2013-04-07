module BomberboysClient
  class Connection 
    def initialize(server)
      @server = server 
    end

    def register(listener)
      @listener = listener 
    end

    def start
      while message = @server.receive
        if @listener.respond_to?(message.action)
          @listener.send(message.action, *message.params)
        else
          puts "ERROR: Listener doesn't respond to #{message.action}."
        end
      end
    end

    def login(name)
      @server.send(Message.new(:login, [name]))
    end

    def move_me(direction)
      dir = { n:0, ne:1, e:2, se:3, s:4, sw:5, w:6, nw:7 }
      @server.send(Message.new(:move_me, [dir[direction]]))
    end

    def plant_bomb
      @server.send(Message.new(:plant_bomb))
    end
  end
end
