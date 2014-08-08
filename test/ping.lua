local nb = require "knewbi"
print("launch ping!!!")
nb.callback(function(data)
	print("call ping back", data)
end)
