import SimpleHTTPServer
import SocketServer

PORT = 8765

class Handler(SimpleHTTPServer.SimpleHTTPRequestHandler):
    pass

Handler.extensions_map['.wasm'] = 'application/wasm'

httpd = SocketServer.TCPServer(("", PORT), Handler)

print "Serving at: http://localhost:" + str(PORT) + "/tool/wasm_demo.html"
httpd.serve_forever()