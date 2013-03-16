module BomberboysClient
  class Server
    def initialize(game = Game.new)
      @actions = {
        "\x00" => :login,    "\x01" => :add_player,   "\x02" => :remove_player,
        "\x03" => :move_me,  "\x04" => :move_player,  "\x05" => :plant_bomb,
        "\x06" => :add_bomb, "\x07" => :explode_bomb, "\x08" => :fall_player
      }
      @game = game
    end

    def accept
    end

    def login(name)
    end

    # direction = :N, :NE, :E, :SE, :S, :SW, :W, :NW
    def move_me(direction)
    end

    def plant_bomb
    end
  end
end
