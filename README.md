# Lua documentation #

## Basics ##

Everytime engine is launched, it will load **logic.lua** file in scripts folder.
This is how **logic.lua** should look like at its simplest form:
```
#!Lua

-- Called when network response is available.
function eventResponse(response)

end

-- Called when GUI component listeners are activated.
function eventGUI(guiNode, eventType)

end

-- Handle initialization.
function start()

end

-- Called when program is brought to foreground.
function resume()

end

-- Called when program screen resolution is changed.
function resize()

end

-- Called every frame.
function update()

end

-- Called when program is brought to background.
function pause()

end

-- Called when program is about to quit.
function quit()

end
```

## Working with 3D models ##

### Loading simple plane with texture ###
```
#!Lua

plane = Model.new("model_name")
plane:setType("plane")
plane:setAmbient("#FFFFFFFF")
plane:setShader(Shader.new("shader_file_name"))
plane:setTexture(Texture.new("texture_file.png"))
plane:setPosY(-2.0)
plane:setScaleXYZ(100.0, 100.0, 100.0)
```
## Enabling physics ##
Physics only work for nodes with already specified **Model** resource!
```
#!Lua
node:setType("plane")
node:enablePhysics()
```