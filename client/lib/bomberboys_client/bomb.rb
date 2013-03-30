module BomberboysClient
  class Bomb
    RADIUS = 150
    attr_reader :id, :x, :y

    def initialize(id, x = 0, y = 0)
      @exploded = false
      @x, @y = x, y
      @id = id
    end

    def position
      [@x, @y]
    end

    def explode
      @exploded = true
    end

    def exploded?
      @exploded
    end

    def to_s
      "{id:#{@id},pos:#{self.position}}"
    end
  end
end
