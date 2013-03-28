module BomberboysClient
  class Bomb
    attr_reader :id, :x, :y

    def initialize(id, x = 0, y = 0, radius = 500)
      @exploded = false
      @x, @y = x, y
      @id = id
      @radius = radius
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

    def explosion_radius
      @radius
    end

    def to_s
      "{id:#{@id},pos:#{self.position}}"
    end
  end
end
