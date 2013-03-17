module BomberboysClient
  class Game
    def initialize
      @board = Board.new
    end

    def add_player(params)
      id   = bytes_to_str(params[0..1])
      x    = bytes_to_str(params[2..3]).to_i
      y    = bytes_to_str(params[4..5]).to_i
      name = bytes_to_str(params[6..-1])

      @board.players[id] = Player.new(id, name, @board.players.empty?)
      @board.players[id].move(x, y)
    end

    def remove_player(params)
      id = bytes_to_str(params[0..1])

      @board.delete(id)
    end

    def move_player(params)
      id   = bytes_to_str(params[0..1])
      x    = bytes_to_str(params[2..3]).to_i
      y    = bytes_to_str(params[4..5]).to_i

      @board.players[id].move(x, y)
    end

    def fall_player(params)
      id = bytes_to_str(params[0..1])

      @board.players[id].kill
    end

    def add_bomb(params)
      id   = bytes_to_str(params[0..1])
      x    = bytes_to_str(params[2..3]).to_i
      y    = bytes_to_str(params[4..5]).to_i

      @board.bombs[id] = Bomb.new(id, x, y)
    end

    def explode_bomb(params)
      id = bytes_to_str(params[0..1])

      @board.bombs[id].explode
    end

    private
    def bytes_to_s(bytes)
      bytes.map { |b| b.chr }.join
    end
  end
end
