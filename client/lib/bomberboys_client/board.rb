module BomberboysClient
  class Board
    attr_reader :players, :bombs

    def initialize
      @players = {}
      @bombs   = {}
    end

    def bomb(id)
      @bombs[id]
    end

    def add_bomb(bomb)
      @bombs[bomb.id] = bomb
    end

    def bombs
      @bombs.values
    end

    def to_s
      "players: #{@players.values}\nbombs: #{@bombs.values}"
    end
  end
end
