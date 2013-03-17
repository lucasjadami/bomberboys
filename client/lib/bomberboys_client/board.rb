module BomberboysClient
  class Board
    attr_reader :players, :bombs

    def initialize
      @players = {}
      @bombs   = {}
    end

    def to_s
      "players: #{@players.values}\nbombs: #{@bombs.values}"
    end
  end
end
