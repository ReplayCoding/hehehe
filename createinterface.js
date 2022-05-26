let client = Process.getModuleByName("client.so")
let createInterfaceFn = client.getExportByName("CreateInterface")

Interceptor.attach(createInterfaceFn, {
  onEnter: function(args) {
    console.log(`
      pName: ${args[0].readCString()}
      pReturnCode*: ${args[1]}
    `)
  },
})
