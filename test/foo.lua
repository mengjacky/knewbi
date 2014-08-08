local obj = Foo(100);
function add(a, b)
	return obj:Add(a, b);
end
function minus(a, b)
	return obj:Minus(a, b);
end
function main()
	local a = add(1, 2);
	local b = minus(10, 5);
	print("add=", a);
	print("minus=", b);
end
main()
