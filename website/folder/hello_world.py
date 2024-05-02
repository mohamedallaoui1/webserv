#!/usr/bin/env python3

import cgi
# while 1:
#     a = "n"
print("Content-type: text/html\n")
print("<html><head><title>CGI POST Test</title></head><body>")
print("<h1>POST Data:</h1>")

form = cgi.FieldStorage()
for key in form.keys():
    print(f"<p>{key}: {form[key].value}</p>")

print("</body></html>")