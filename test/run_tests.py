import subprocess

class CommandLineOptions():
	VAULT_OPTION = '-v'
	KEY_OPTION = '-k'
	NAME_OPTION = '-n'
	USERNAME_OPTION = '--un'
	PASSWORD_OPTION = '--pw'
	NOTE_OPTION = '--note'
	CLIP_OPTION = '-c'
	PRINT_OPTION = '-p'
	UPDATE_OPTION = '-u'
	NEWKEY_OPTION = '--knew'
	FILE_OPTION = '-f'
	DELETE_OPTION = '-d'
	ADD_OPTION = '-a'
	INFO_OPTION = '-i'

class Account:
	def __init__(self, tag, username, password, note):
		self.tag = tag
		self.username = username
		self.password = password
		self.note = note

## Executes the given command and returns the output void of trailing whitespace.
def exec_cmd(cmd):
	return subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE).stdout.read().decode('UTF-8').rstrip('\t\n ')

def construct_cmd(args):
	return ''.join([arg + ' ' for arg in args]).rstrip(' ')

def test_vault():
	# tests vault commands
	print("Hello world!")

def add_vault_command(exec, new_vault_name, new_vault_key):
	return exec_cmd(construct_cmd([
		exec,
		CommandLineOptions.VAULT_OPTION,
		'add',
		CommandLineOptions.NAME_OPTION,
		new_vault_name,
		CommandLineOptions.KEY_OPTION,
		new_vault_key]))

def update_vault_command(exec, old_key, new_key):
	print("Hello world!")

def switch_vault_command(exec, vault_name, vault_key):
	print("Hello world!")

def delete_vault_command(exec, vault_name, vault_key):
	print("Hello world!")

def list_command(exec, vault_key=None, info=False):
	if vault_key and info:
		cmd = construct_cmd([
			exec,
			CommandLineOptions.VAULT_OPTION,
			'list', CommandLineOptions.KEY_OPTION,
			vault_key,
			CommandLineOptions.INFO_OPTION])
	elif vault_key:
		cmd = construct_cmd([
			exec,
			CommandLineOptions.VAULT_OPTION,
			'list',
			CommandLineOptions.KEY_OPTION,
			vault_key])
	else:
		cmd = construct_cmd([
			exec,
			CommandLineOptions.VAULT_OPTION,
			'list'])
	return exec_cmd(cmd)

def test_print():
	# tests print commands
	print("Hello world!")

def print_command(exec, account_name, vault_key, option=None):
	print("Hello world!")

def test_clip():
	# tests clip commands
	print("Hello world!")

def clip_command(exec, account_name, vault_key, option):
	print("Hello world!")

def test_update():
	# tests update commands
	print("Hello world!")

def update_command(exec, account_name, vault_key, option, arg=None):
	print("Hello world!")

def test_add():
	# tests add commands
	print("Hello world!")

def add_command(exec, account_name, vault_key, file_path=None, un=None, pw=None):
	print("Hello world!")

if __name__ == "__main__":
	print(list_command('./pml', 'pass123', True))
