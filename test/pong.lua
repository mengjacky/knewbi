local nb = require("knewbi")
print("launch pong!!!")
nb.callback(function(data)
	print("call pong back", data)
end)
