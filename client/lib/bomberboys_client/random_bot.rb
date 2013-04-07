module BomberboysClient
  class RandomBot
    DIRECTIONS = [:ne, :n, :nw, :w, :sw, :s, :se, :e] 

    def initialize(board, connection)
      @board = board
      @connection = connection
    end

    def play
      loop do
        10.times { move_me DIRECTIONS.sample }
        plant_bomb
      end
    end

    private
    def move_me(direction)
      @connection.move_me(direction)
      sleep 0.2
    end

    def plant_bomb
      @connection.plant_bomb
    end
  end
end
