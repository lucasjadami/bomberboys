module BomberboysClient
  class LocalPlayer 
    DIRECTIONS = [:ne, :n, :nw, :w, :sw, :s, :se, :e] 

    def initialize(server)
      @server = server
    end

    def play
      @thread = Thread.new { loop { react @board } }
    end

    def react(world); end

    def join
      @thread.join
    end

    protected
    def move_me(direction)
      dir = { n:0, ne:1, e:2, se:3, s:4, sw:5, w:6, nw:7 }
      @server.send(Message.new(:move_me, [dir[direction]]))
      sleep 0.2
    end

    def plant_bomb
      @server.send(Message.new(:plant_bomb))
    end
  end
end
