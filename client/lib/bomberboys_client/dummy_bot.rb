module BomberboysClient
  class DummyBot
    DIRECTIONS = [:ne, :n, :nw, :w, :sw, :s, :se, :e] 

    def initialize(board, connection)
      @board = board
      @connection = connection
    end

    def play
      10.times { move_me DIRECTIONS.sample }
      plant_bomb

      loop do
        if local.x > 550
          move_me :w
        elsif local.x < 50
          move_me :e
        elsif local.y > 370
          move_me :s
        elsif local.y < 50
          move_me :n
        elsif bomb = @board.dangerous_bombs.first
          move_me opposite_direction(bomb)
        elsif @board.attackable_players.any?
          plant_bomb
          move_me(DIRECTIONS.sample)
        elsif (player = @board.nearest_players.first) && @board.local_bomb && @board.local_bomb.exploded?
          move_me direction(player)
        end
      end
    end

    private
    def opposite_direction(bomb)
      cardinal_direction(Math.atan2(bomb.y - local.y, bomb.x - local.x) + Math::PI)
    end

    def direction(obj)
      cardinal_direction(Math.atan2(obj.y - local.y, obj.x - local.x))
    end

    def cardinal_direction(angle)
      DIRECTIONS[(4*angle/Math::PI - 0.5).ceil - 1]
    end

    def move_me(direction)
      @connection.move_me(direction)
      sleep 0.5
    end

    def plant_bomb
      @connection.plant_bomb
    end

    def local
      @board.local
    end
  end
end
