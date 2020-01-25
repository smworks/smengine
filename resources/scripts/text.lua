function eventResponse(response)
end

function eventGUI(guiNode, eventType)
	print(guiNode:getName() .. ".lua")
end

function start()
	print("Called start()")
	
	helloWorldLabel = Text.new("helloWorldLabel")
	helloWorldLabel:setText("Hello world")
	helloWorldLabel:setShader(Shader.new("surface_bckg_color"))
	
end

function resume()
	print("Called resume()")
end

function resize()
	print("Called resize()")
end

function update()
	if input:keyReleased(constants["N"]) then
		loadScene("main.lua")
		print "Loading main.lua"
	end
end

function pause()
	print("Called pause()")
end

function quit()
	print("Called quit()")
end