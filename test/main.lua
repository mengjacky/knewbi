local nb = require "knewbi"
print("start finish");

function main(data)
	print("main function")
	nb.call("ping", "hello ping")
end

nb.reg("ping", "test/ping.lua")
nb.reg("pong", "test/pong.lua")
nb.callback(main)
