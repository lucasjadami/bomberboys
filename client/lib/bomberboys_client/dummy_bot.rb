module BomberboysClient
  class DummyBot
    DIRECTIONS = [:ne, :n, :nw, :w, :sw, :s, :se, :e] 

    def initialize(name, board, connection)
      @board = board
      @connection = connection
      @name = name
    end

    def play
      login

      10.times { |t| puts t; move_me(DIRECTIONS.sample) }
      plant_bomb

      # loop do
      #   bombs = dangerous_bombs
      #   if bombs.empty?
      #     plant_bomb
      #   else
      #     move_me opposite_direction(bombs.first)
      #   end
      # end
    end

    private
    def most_dangerous_bomb(bombs)
    end

    def dangerous_bombs
      sorted_bombs = @board.bombs.sort do |b1, b2|
        distance(local.position, b1.position) <=> distance(local.position, b2.position)
      end

      sorted_bombs.select do |b|
        distance(b.position, local.position) < b.explosion_radius
      end
    end

    def opposite_direction(bomb)
      if local.y == bomb.y 
        DIRECTIONS.sample
      else 
        opposite_angle = Math.atan2(bomb.y - local.y, bomb.x - local.x) + Math::PI
        direction_index = (4*opposite_angle/Math::PI - 0.5).ceil - 1

        DIRECTIONS[direction_index]
      end
    end

    def distance(obj1, obj2)
      pairs = obj1.zip(obj2)
      sum = pairs.reduce(0) { |sum, el| sum + (el[0] - el[1])**2 }

      Math.sqrt(sum)
    end

    def move_me(direction)
      @connection.move_me(direction)
      sleep 0.1
    end

    def plant_bomb
      @connection.plant_bomb
    end

    def login
      @connection.login(@name)
    end

    def local
      @board.local_player
    end
  end
end
