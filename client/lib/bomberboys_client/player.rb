module BomberboysClient
  class Player
    attr_reader :name, :id
    attr_reader :local

    def initialize(id, name)
      @x = @y = 0
      @dead = false
      @name = name
      @id = id
    end

    def move(x, y)
      @x, @y = x, y
    end

    def position
      [@x, @y]
    end

    def kill
      @dead = true
    end

    def dead?
      @dead
    end

    def local?
      @local
    end

    def to_s
      "{id:#{@id},pos:#{self.position},local:#{self.local?},dead:#{self.dead?}}"
    end
  end
end
