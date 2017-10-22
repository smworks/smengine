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

## Camera controls ##

### Moving along camera directions and looking around ###

```
#!Lua

function update()
	camera = getCamera()
	input = getInput()

	local offset = 0.1
	if input:keyPressed(constants["W"]) then
		camera:moveZ(-offset)
	end
	if input:keyPressed(constants["S"]) then
		camera:moveZ(offset)
	end
	if input:keyPressed(constants["A"]) then
		camera:moveX(-offset)
	end
	if input:keyPressed(constants["D"]) then
		camera:moveX(offset)
	end

    camera:addRotX(input:getPointerDeltaY() * 0.2)
    camera:addRotY(input:getPointerDeltaX() * 0.2)
end

```
## Working with 2D models ##

### Loading simple sprite with texture ###
```
#!Lua
texture = Texture.new("texture.png")
sprite = Sprite.new("Sprite name")
sprite:setWidth(200);
sprite:setHeight(200);
sprite:setPosX(300)
sprite:setPosY(300)
sprite:setTexture(texture)
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
Physics only work for nodes with **Model** resource.
Call **enablePhysics()** after setting model type and other attributes.
```
#!Lua
node:setType("plane")
node:setScaleXYZ(10.0, 1.0, 3.0)
node:enablePhysics()
```