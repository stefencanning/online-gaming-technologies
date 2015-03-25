import tornado.auth
import tornado.escape
import tornado.httpserver
import tornado.ioloop
import tornado.options
import tornado.web
import base64

from tornado.options import define, options
from twython import Twython

define("port", default=8888, help="run on the given port", type=int)

#TO DO: change this
origin = "http://52.17.79.20:8000"
callback_url = origin+"/test.html"

twitter=None

#Twiz test app
APP_KEY = CONSUMER_KEY = 'fbid4njYC9sd4UzGq2RdnpvUx'
APP_SECRET = CONSUMER_SECRET= 'NE30Vx2ifgH3syO3w0OxYewrzKMexHH2eoeckq3QOnK14LdPqf'
OAUTH_TOKEN=""
OAUTH_TOKEN_SECRET=""

class BaseHandler(tornado.web.RequestHandler):
	def options(self):
		self.set_header("Access-Control-Allow-Origin",origin)

		#The key line that took a very very long time to find
		self.set_header('Access-Control-Allow-Headers', 'Authorization' )
		
		self.set_header('Access-Control-Allow-Credentials', "true")
	

class OAuthHandler(BaseHandler):

	def get(self):
		print("OAuthHandler")
		self.set_header("Access-Control-Allow-Origin",origin)
		self.set_header('Access-Control-Allow-Headers', 'Authorization' )
		self.set_header('Access-Control-Allow-Credentials', "true")
  
		oauth_token = ""

		try:
			oauth_token = self.get_argument("oauth_token")
			oauth_verifier = self.get_argument("oauth_verifier")
			
			self.step2() 

		#if the parameters aren't there, means step 1
		except tornado.web.MissingArgumentError:
			print("Step 1")
			self.step1()	  
	 
	   
	def step1(self):
		print("step1")
		self.set_header("Access-Control-Allow-Origin",origin)
		self.set_header('Access-Control-Allow-Headers', 'Authorization' )
		self.set_header('Access-Control-Allow-Credentials', "true")
		
		global twitter
		twitter = Twython(CONSUMER_KEY, APP_SECRET)
		auth = twitter.get_authentication_tokens(callback_url=callback_url)

		global OAUTH_TOKEN
		global OAUTH_TOKEN_SECRET
		OAUTH_TOKEN = auth['oauth_token']
		OAUTH_TOKEN_SECRET = auth['oauth_token_secret']

		print("step 1")   
		self.write(auth['auth_url'])		
		print(OAUTH_TOKEN)
		print(OAUTH_TOKEN_SECRET)



	def step2(self):
		print("step2")
		self.set_header("Access-Control-Allow-Origin",origin)
		self.set_header('Access-Control-Allow-Headers', 'Authorization' )
		self.set_header('Access-Control-Allow-Credentials', "true")
			  
		
		print("Step2")
		oauth_token = self.get_argument("oauth_token")
		oauth_verifier = self.get_argument("oauth_verifier")

		print("Step 2: oauth_token: " + oauth_token)
		print("Step 2: oauth_verifier: " + oauth_verifier)

		global OAUTH_TOKEN
		global OAUTH_TOKEN_SECRET
		global twitter
		twitter = Twython(APP_KEY, APP_SECRET,
				  OAUTH_TOKEN, OAUTH_TOKEN_SECRET)

		final_step = twitter.get_authorized_tokens(oauth_verifier)

		OAUTH_TOKEN = final_step['oauth_token']
		OAUTH_TOKEN_SECRET = final_step['oauth_token_secret']
		print(OAUTH_TOKEN)
		twitter = Twython(APP_KEY, APP_SECRET,
				  OAUTH_TOKEN, OAUTH_TOKEN_SECRET)
		screen_name = twitter.verify_credentials()["screen_name"]
		twitter.update_status(status='online gaming lab 7 test tweet')
		self.write(screen_name)
	   
	  
	



class Application(tornado.web.Application):
	def __init__(self):
		handlers = [
			
			(r"/oauth/", OAuthHandler),
			
			
		]
	  
		tornado.web.Application.__init__(self, handlers)

def main():

	tornado.options.parse_command_line()
	http_server = tornado.httpserver.HTTPServer(Application())
	http_server.listen(options.port)
	tornado.ioloop.IOLoop.instance().start()

if __name__ == "__main__":
	main()