import json
import cPickle
import base64


accounts = {
"accounts"	:	[
					{
						'username': 'Duy',
						'password': 'abc',
						'keyFile' : 'Duy_public_key.pem'
					},
					{
						'username': 'Billy',
						'password': 'abc',
						'keyFile' : 'Billy_public_key.pem'
					},
					{
						'username': 'Holly',
						'password': 'abc',
						'keyFile' : 'Holly_public_key.pem'
					},
					{
						'username': 'Tevin',
						'password': 'abc',
						'keyFile' : 'Tevin_public_key.pem'
					},
				]
}

# Convert accounts into binary string
serializedData = cPickle.dumps(accounts)

# Encode with base64
encodedData = base64.b64encode(serializedData)

# write to file
with open('accounts.json', 'w') as f:
	f.write(encodedData)
	print "Done"
	
	
	
		

	
