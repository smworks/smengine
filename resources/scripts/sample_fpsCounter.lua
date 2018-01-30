function eventResponse(response)
end

function eventGUI(node, eventType)
end

function start()
	print("Called start()")
	input = getInput()
    print("Screen width in lua: " .. getScreenWidth() .. ", height: " .. getScreenHeight())
    fpsCounter = Button.new("fpsCounter")
    fpsCounter:setColor("#FFFF00FF")
    fpsCounter:setBackground("#33333333")
    fontSize = 14
    fpsCounter:setFontSize(14)
    fpsCounter:setWidth(1256)
    fpsCounter:setHeight(64)
    fpsCount = 0
    fpsTime = 0
end

function resume()
	print("Called resume()")
end

function resize()
	print("Called resize()")
end

function update()
    fpsTime = fpsTime + getTimeDelta()
    if fpsTime >= 1000 then
        fpsCounter:setText("FPS: " .. fpsCount .. "\nResolution: " .. getScreenWidth() .. "x" .. getScreenHeight() .. "px")
        fpsCount = 0
        fpsTime = 0
    end
end

function pause()
	print("Called pause()")
end

function quit()
	print("Called quit()")
end