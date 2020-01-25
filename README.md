# Lua documentation #

## Basics ##

Everytime engine is launched, it will load **logic.lua** file in scripts folder.
This is how **logic.lua** should look like at its simplest form:
```Lua

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

## General functions ##
```lua
print(text) -- used for printing text
pointerIsOver(node) -- returns true if mouse or other pointer is over specific node
playSound("file.mp3", true) -- plays sound file. Second parameter indicates 
loadScene("scene.lua") -- loads specified lua scene file
exit() -- closes program
getScreenWidth() -- returns screen width in pixels
getScreenHeight() -- returns screen height in pixels
getTimeDelta() -- returns elapsed time in milliseconds
```

## Camera controls ##

### Moving along camera directions and looking around ###

```Lua
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
## Working with nodes ##
Every object in the scene belongs to a **Node** object.
As a result all of these methods can be called on each object:
```Lua
node = getNode("resource name") -- node and it's corresponding resource share same name
node:setParent(parentNode)
node:addChild(childNode)
node:enablePhysics(massInKg) -- enables physics. Can be used only on Model resources
node:disablePhysics()
node:setMass(99.9) -- sets physics mass in kg
node:accelerate(0.0, 100.0, 0.0) -- accelerates physics object in specified vector direction and speed
node:setBool("paramName", true) -- sets attribute for underlying resource
node:setInt("paramName", 5) -- sets attribute for underlying resource
node:setFloat("paramName", 5.5) -- sets attribute for underlying resource
node:setString("paramName", "paramValue") -- sets attribute for underlying resource
node:setShader(shaderObject) -- sets shader. Applies to model and sprite only.
shader = node:getShader(shaderObject) -- returns shader used by resource
node:setTexture(textureObject) -- applies to model or sprite only.
node:setIndex(spriteIndex) -- allows to specify animation frame for animated sprite resources.
node:getCount() -- returns number of frames in sprite resources.
node:getName() -- returns node name
node:setVisibility(true) -- sets visibility flag to indicate whether node should be rendered
isNodeVisible = node:getVisibility() -- returns visibility flag
node:setFontSize(16) -- sets text size for Text and Button nodes
node:setText("hello world") -- sets text for Text and Button nodes
node:setColor("#FFFFFFFF") -- sets text color for Text and Button nodes. Format - ARGB
node:setBackground("#FF000000") -- sets background for any GUISurface (Button, Text)
node:setBackground(textureObject) -- sets background texture for any GUISurface (Button, Text)
backgroundColor = node:getBackground() -- returns background color of GUISurface
nodeWidth = node:getWidth() -- returns width, same as node:getScaleX()
nodeHeight = node:getHeight() -- returns height, same as node:getScaleY()
node:setWidth(500) -- sets node width. Same as node:setScaleX(500)
node:setHeight(500) -- sets node height. Same as node:setScaleY(500)
node:setAmbient("#FFFFFFFF") -- sets ambient color. Applies to model resources only.
node:setDiffuse("#FFFFFFFF") -- sets diffuse color. Applies to model resources only.
node:setSpecular("#FFFFFFFF") -- sets specular color. Applies to model resources only.
node:setType("mesh") -- sets model type(mesh|shape|plane|sphere|terrain|water). Applies to model resources only.
node:moveX(100.0) -- move node on it's direction X axis
node:moveY(100.0) -- move node on it's direction Y axis
node:moveZ(100.0) -- move node on it's direction Z axis
node:moveViewCameraDirection(100.0) -- move node in camera look at direction.
node:(add|set|get)(Pos|Scale|Rot)(X|Y|Z)(100) -- use any combination of method parts to construct required call. XYZ can be combined together, or in parts of XY, YZ, XZ.

```

## Working with 2D models ##

### Loading simple sprite with texture ###
```Lua
texture = Texture.new("texture.png")
sprite = Sprite.new("Sprite name")
sprite:setWidth(200);
sprite:setHeight(200);
sprite:setPosX(300)
sprite:setPosY(300)
sprite:setTexture(texture)
```

### Loading simple text ###
```Lua
text = Text.new("text name")
text:setText("Hello world")
text:setColor("#CC0000FF")
text:setPosX(200) -- Position from left
text:setPosY(50) -- Position from top
text:setFontSize(46)
text:setBackground("#000000FF")
text:setVisibility(true)
```

## Working with 3D models ##

### Loading simple plane with texture ###
```Lua
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
```Lua
node:setType("plane")
node:setScaleXYZ(10.0, 1.0, 3.0)
node:enablePhysics()
```
