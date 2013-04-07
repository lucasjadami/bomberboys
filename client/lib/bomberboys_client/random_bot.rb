module BomberboysClient
  class RandomBot
    DIRECTIONS = [:ne, :n, :nw, :w, :sw, :s, :se, :e] 

    def initialize(board, connection)
      @board = board
      @connection = connection
    end

    def play
      @thread = Thread.new do
        loop do
          plant_frequency.times { move_me DIRECTIONS.sample }
          plant_bomb
        end
      end
    end

    def join
      @thread.join
    end

    private
    def move_me(direction)
      @connection.move_me(direction)
      sleep 0.2
    end

    def plant_bomb
      @connection.plant_bomb
    end

    def plant_frequency
      10 + rand(20)
    end
  end
end
