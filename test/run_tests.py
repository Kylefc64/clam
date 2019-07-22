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

def clean_dir():
    exec_cmd(construct_cmd(['rm', '-rf', 'vaults', 'meta']))

def test_vault():
    # tests vault commands
    clean_dir()

    exec = './pml'

    add_vault_command(exec, 'vault1', 'key1')
    add_vault_command(exec, 'vault2', 'key2')
    add_vault_command(exec, 'vault3', 'key3')

    add_command(exec, 'acct1-1', 'key1', 'un1-1', 'pw1-1', None)
    add_command(exec, 'acct1-2', 'key1', 'un1-2', 'pw1-2', None)
    add_command(exec, 'acct1-3', 'key1', 'un1-3', 'pw1-3', None)

    switch_vault_command(exec, 'vault2', 'key2')
    add_command(exec, 'acct2-1', 'key2', 'un2-1', 'pw2-1', None)
    add_command(exec, 'acct2-2', 'key2', 'un2-2', 'pw2-2', None)
    add_command(exec, 'acct2-3', 'key2', 'un2-3', 'pw2-3', None)

    switch_vault_command(exec, 'vault3', 'key3')
    add_command(exec, 'acct3-1', 'key3', 'un3-1', 'pw3-1', None)
    add_command(exec, 'acct3-2', 'key3', 'un3-2', 'pw3-2', None)
    add_command(exec, 'acct3-3', 'key3', 'un3-3', 'pw3-3', None)

    print(list_command(exec)) # assertion here
    print(list_command(exec, 'key3')) # assertion here
    print(list_command(exec, 'key3', info=True)) # assertion here

    update_vault_command(exec, 'key3', 'key3new')
    print(list_command(exec, 'key')) # assertion here (invalid key)
    print(list_command(exec, 'key3new')) # assertion here

    print(delete_vault_command(exec, 'vault3', 'key3new')) # assertion here (cannot delete active vault)
    delete_vault_command(exec, 'vault2', 'key2')
    print(list_command(exec)) # assertion here

    clean_dir()

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
    return exec_cmd(construct_cmd([
        exec,
        CommandLineOptions.VAULT_OPTION,
        'update',
        CommandLineOptions.KEY_OPTION,
        old_key,
        CommandLineOptions.NEWKEY_OPTION,
        new_key]))

def switch_vault_command(exec, vault_name, vault_key):
    return exec_cmd(construct_cmd([
        exec,
        CommandLineOptions.VAULT_OPTION,
        'switch',
        CommandLineOptions.NAME_OPTION,
        vault_name,
        CommandLineOptions.KEY_OPTION,
        vault_key]))

def delete_vault_command(exec, vault_name, vault_key):
    return exec_cmd(construct_cmd([
        exec,
        CommandLineOptions.VAULT_OPTION,
        'delete',
        CommandLineOptions.NAME_OPTION,
        vault_name,
        CommandLineOptions.KEY_OPTION,
        vault_key]))

def list_command(exec, vault_key=None, info=False):
    if vault_key and info:
        cmd = construct_cmd([
            exec,
            CommandLineOptions.VAULT_OPTION,
            'list',
            CommandLineOptions.KEY_OPTION,
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
    clean_dir()

    exec = './pml'

    add_command(exec, 'acct1', 'key1', 'un1-1', 'pw1-1', None)
    update_command(exec, 'acct1', 'key1', CommandLineOptions.NOTE_OPTION, 'note1')

    print(print_command(exec, 'acct1', 'key1')) # assertion here
    print(print_command(exec, 'acct1', 'key1', CommandLineOptions.USERNAME_OPTION)) # assertion here
    print(print_command(exec, 'acct1', 'key1', CommandLineOptions.PASSWORD_OPTION)) # assertion here
    print(print_command(exec, 'acct1', 'key1', CommandLineOptions.NOTE_OPTION)) # assertion here

def print_command(exec, account_name, vault_key, option=None):
    if option is None:
        cmd = construct_cmd([
            exec,
            CommandLineOptions.PRINT_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key])
    else:
        cmd = construct_cmd([
            exec,
            CommandLineOptions.PRINT_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key,
            option])
    return exec_cmd(cmd)

def test_clip():
    # tests clip commands
    print("Hello world!")

def clip_command(exec, account_name, vault_key, option):
    return exec_cmd(construct_cmd([
            exec,
            CommandLineOptions.CLIP_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key,
            option]))

def test_update():
    # tests update commands
    clean_dir()

    exec = './pml'

    add_command(exec, 'acct1', 'key1', 'un1-1', 'pw1-1', None)
    update_command(exec, 'acct1', 'key1', CommandLineOptions.NOTE_OPTION, 'note1')

    print(print_command(exec, 'acct1', 'key1', CommandLineOptions.USERNAME_OPTION)) # assertion here
    print(print_command(exec, 'acct1', 'key1', CommandLineOptions.PASSWORD_OPTION)) # assertion here
    print(print_command(exec, 'acct1', 'key1', CommandLineOptions.NOTE_OPTION)) # assertion here

    update_command(exec, 'acct1', 'key1', CommandLineOptions.USERNAME_OPTION, 'un-new')
    update_command(exec, 'acct1', 'key1', CommandLineOptions.PASSWORD_OPTION, 'pw-new')
    update_command(exec, 'acct1', 'key1', CommandLineOptions.NOTE_OPTION, 'note-new')

    print(print_command(exec, 'acct1', 'key1', CommandLineOptions.USERNAME_OPTION)) # assertion here
    print(print_command(exec, 'acct1', 'key1', CommandLineOptions.PASSWORD_OPTION)) # assertion here
    print(print_command(exec, 'acct1', 'key1', CommandLineOptions.NOTE_OPTION)) # assertion here

    add_command(exec, 'acct2', 'key1', 'un2-1', 'pw2-1', None)
    print(list_command(exec, 'key1')) # assertion here
    update_command(exec, 'acct1', 'key1', CommandLineOptions.DELETE_OPTION)
    print(list_command(exec, 'key1')) # assertion here

def update_command(exec, account_name, vault_key, option, arg=None):
    if arg is None:
        cmd = construct_cmd([
            exec,
            CommandLineOptions.UPDATE_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key,
            option])
    else:
        cmd = construct_cmd([
            exec,
            CommandLineOptions.UPDATE_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key,
            option,
            arg])
    return exec_cmd(cmd)

def test_add():
    # tests add commands
    print("Hello world!")

def add_command(exec, account_name, vault_key, un=None, pw=None, file_path=None):
    if file_path is not None:
        cmd = construct_cmd([
            exec,
            CommandLineOptions.ADD_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key,
            CommandLineOptions.FILE_OPTION,
            file_path])
    elif un is not None and pw is not None:
        cmd = construct_cmd([
            exec,
            CommandLineOptions.ADD_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key,
            CommandLineOptions.USERNAME_OPTION,
            un,
            CommandLineOptions.PASSWORD_OPTION,
            pw])
    else:
        cmd = construct_cmd([
            exec,
            CommandLineOptions.ADD_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key])
    return exec_cmd(cmd)

if __name__ == "__main__":
    #test_vault()
    #test_print()
    test_update()
