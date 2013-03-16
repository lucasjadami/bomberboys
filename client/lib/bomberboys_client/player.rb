class Player
  attr_reader :name, :id
  attr_reader :local

  def initialize(id, name, local = false)
    @x = @y = 0
    @dead = false
    @name = name
    @id = id
    @local = local
  end

  def move(x, y)
    @x, @y = x, y
  end

  def position
    [@x, @y]
  end

  def die
    @dead = true
  end

  def dead?
    @dead
  end

  def local?
    @local
  end
end
