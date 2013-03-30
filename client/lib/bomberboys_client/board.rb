module BomberboysClient
  class Board
    attr_reader :players, :bombs, :local_player

    def initialize
      @players = {}
      @bombs   = {}
    end

    def add_player(player)
      @local_player = player if @players.empty?
      @players[player.id] = player
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
