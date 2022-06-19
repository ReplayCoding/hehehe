let client = Process.getModuleByName("engine.so").base
let readDemoHeader = client.add("0x003494c0")
let testMod = Module.load("/home/user/Projects/tf2_hacking_memes/frida-c/test.so")
let testFunc_addr = testMod.findExportByName("testing_func")
let testFunc = new NativeFunction(testFunc_addr, "void", [])

testFunc()
