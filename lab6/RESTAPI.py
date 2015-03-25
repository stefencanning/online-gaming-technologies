from datetime import date
import tornado.escape
import tornado.ioloop
import tornado.web
import mysql.connector
import json
 
class VersionHandler(tornado.web.RequestHandler):
	def get(self):
		response = { 'version': '3.5.1',
					 'last_build':  date.today().isoformat() }
		self.write(response)
 
class GetGameByIdHandler(tornado.web.RequestHandler):
	def get(self, id):
		response = { 'id': int(id),
					 'name': 'Crazy Game',
					 'release_date': date.today().isoformat() }
		self.write(response)

class GetUsers(tornado.web.RequestHandler):
	def get(self):
		cnx = mysql.connector.connect(user='root', passwd='ichigo7340', database='saveDatabase')
		cursor = cnx.cursor()

		cursor.execute("select * from usernames")
		# Fetch a single row using fetchone() method.
		response ={}
		for(id,name) in cursor:
			response[id]=name
		print(response)
		#return json.dumps(response)
		self.write(response)
		# disconnect from server
		cursor.close()
		cnx.close()

class AddUser(tornado.web.RequestHandler):
	def post(self):
		id = self.get_argument('id', -1)
		name = self.get_argument('name', '')
		
		print(id)
		print(name)
		if not id:
			login_response = {
				'error': True, 
				'msg': 'Please enter your email address.'
				}
		elif not name:
			login_response = {
				'error': True, 
				'msg': 'Please enter your password.'
			}
		else:
			# Open database connection
			cnx = mysql.connector.connect(user='root', passwd='ichigo7340', database='saveDatabase')

			# prepare a cursor object using cursor() method
			cursor = cnx.cursor()

			# Prepare SQL query to INSERT a record into the database.
			sql = "INSERT INTO usernames(id, name) VALUES ('%d', '%s')" %(int(float(id)), name)
			try:
				# Execute the SQL command
				cursor.execute(sql)
				# Commit your changes in the database
				cnx.commit()
				login_response = {
					'error': True, 
					'msg': 'success.'
				}
			except:
				# Rollback in case there is any error
				cnx.rollback()
				login_response = {
					'error': True, 
					'msg': 'failed.'
				}


			# disconnect from server
			cursor.close()
			cnx.close()

		
		#self.write(login_response)
 
application = tornado.web.Application([
	(r"/getgamebyid/([0-9]+)", GetGameByIdHandler),
	(r"/version", VersionHandler),
	(r"/addUser", AddUser),
	(r"/getUsers", GetUsers)
])
 
if __name__ == "__main__":
	application.listen(8000)
	tornado.ioloop.IOLoop.instance().start()