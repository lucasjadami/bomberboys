module BomberboysClient
  class Board
    attr_reader :remote_players, :bombs
    attr_accessor :local_player

    def initialize
      @players = {}
      @bombs   = {}
    end
  end
end
