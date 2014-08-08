local nb = require "knewbi"
--thread , main
nb.start(1, "test/main.lua")
nb.loopwait(6);
