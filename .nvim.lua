-- Build & run without debugging
vim.keymap.set("n", "<leader>r", function()
	print("Mapping works!")
	vim.cmd("!cmake --build build --config Debug --target BytecodeVM && ./build/bin/BytecodeVM file.txt")
end, { desc = "Build & run Bytecode VM" })
-- Force load dap and its UI
vim.api.nvim_create_user_command("Deb", function()
	local ok, dap = pcall(require, "dap")
	if not ok then
		vim.notify("nvim-dap not loaded yet")
		return
	end

	dap.run({
		type = "gdb",
		request = "launch",
		name = "Debug BytecodeVM",
		program = vim.fn.getcwd() .. "/build/bin/BytecodeVM",
		args = { vim.fn.input("Program args: ") },
		cwd = vim.fn.getcwd(),
		stopAtEntry = false,
		setupCommands = { { text = "-enable-pretty-printing", ignoreFailures = true } },
	})
end, {})
