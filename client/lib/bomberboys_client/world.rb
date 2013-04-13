module BomberboysClient
  class World 
    attr_reader :players
    attr_reader :bombs
    attr_reader :local_id

    def initialize
      @players = {}
      @bombs   = {}
    end

    def add_player(id, x, y, name)
      player = Player.new(id, name)
      player.move(x, y)

      @local_id = id if @players.empty?
      @players[player.id] = player
    end

    def remove_player(id)
      @players.delete(id)
    end

    def move_player(id, x, y)
      @players[id].move(x, y) unless @players[id].nil?
    end

    def nearest_players
      players = @players.values - [local]

      players.sort { |a, b| distance(local.position, a.position) <=> distance(local.position, b.position) }
    end

    def attackable_players
      nearest_players.select { |b| distance(b.position, local.position) < Bomb::RADIUS }
    end

    def fall_player(id)
      @players[id].kill unless @players[id].nil?
    end

    def add_bomb(id, x, y)
      @bombs[id] = Bomb.new(id, x, y)
    end

    def local_bomb
      @bombs[@local_id]
    end

    def explode_bomb(id)
      @bombs[id].explode
    end

    def bombs
      @bombs.values
    end

    def dangerous_bombs
      self.bombs.select do |b|
        distance(b.position, local.position) < Bomb::RADIUS && !b.exploded?
      end
    end

    def local
      @players[@local_id]
    end

    private
    def distance(obj1, obj2)
      pairs = obj1.zip(obj2)
      sum = pairs.reduce(0) { |sum, el| sum + (el[0] - el[1])**2 }

      Math.sqrt(sum)
    end
  end
end
