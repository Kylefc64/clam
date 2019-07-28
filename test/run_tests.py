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

class TestSuite:
    def __init__(self, test_name):
        self.test_name = test_name
        self.total = 0
        self.passed = 0

    def assert_equals(self, expected, actual):
        self.total += 1
        #print("Expected: " + expected)
        #print("Actual: " + actual)
        if expected == actual:
            self.passed += 1
        else:
            print("Test " + str(self.total) + " failed. Expected: " + expected + " Actual: " + actual)
    
    def finish(self):
        print("Test suite [" + self.test_name + "] finished. " + str(self.passed) + '/' + str(self.total) + " tests passed.")

class Account:
    def __init__(self, tag, username, password, note):
        self.tag = tag
        self.username = username
        self.password = password
        self.note = note

"""
    Given the expected lines, builds a string containing all of those lines separated by newlines.
"""
def build_console_output(*lines):
    return ''.join([line + '\n' for line in lines]).rstrip('\n')

## Executes the given command and returns the output void of trailing whitespace.
def exec_cmd(cmd):
    return subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE).stdout.read().decode('UTF-8').rstrip('\t\n ')

def construct_cmd(*args):
    return ''.join([arg + ' ' for arg in args]).rstrip(' ')

def clean_dir():
    exec_cmd(construct_cmd('rm', '-rf', '~/.pml'))

def test_vault():
    # tests vault commands
    clean_dir()

    exec = './pml'

    vault1_name, vault1_key = 'vault1', 'key1'
    vault2_name, vault2_key = 'vault2', 'key2'
    vault3_name, vault3_key, vault3_newkey = 'vault3', 'key3', 'key3new'

    vault1_acct1_tag, vault1_acct1_un, vault1_acct1_pw = 'acct1-1', 'un1-1', 'pw1-1'
    vault1_acct2_tag, vault1_acct2_un, vault1_acct2_pw = 'acct1-2', 'un1-2', 'pw1-2'
    vault1_acct3_tag, vault1_acct3_un, vault1_acct3_pw = 'acct1-3', 'un1-3', 'pw1-3'

    vault2_acct1_tag, vault2_acct1_un, vault2_acct1_pw = 'acct2-1', 'un2-1', 'pw2-1'
    vault2_acct2_tag, vault2_acct2_un, vault2_acct2_pw = 'acct2-2', 'un2-2', 'pw2-2'
    vault2_acct3_tag, vault2_acct3_un, vault2_acct3_pw = 'acct2-3', 'un2-3', 'pw2-3'

    vault3_acct1_tag, vault3_acct1_un, vault3_acct1_pw = 'acct3-1', 'un3-1', 'pw3-1'
    vault3_acct2_tag, vault3_acct2_un, vault3_acct2_pw = 'acct3-2', 'un3-2', 'pw3-2'
    vault3_acct3_tag, vault3_acct3_un, vault3_acct3_pw = 'acct3-3', 'un3-3', 'pw3-3'

    test_suite = TestSuite('test_vault')

    add_vault_command(exec, vault1_name, vault1_key)
    add_vault_command(exec, vault2_name, vault2_key)
    add_vault_command(exec, vault3_name, vault3_key)

    add_command(exec, vault1_acct1_tag, vault1_key, vault1_acct1_un, vault1_acct1_pw, None)
    add_command(exec, vault1_acct2_tag, vault1_key, vault1_acct2_un, vault1_acct2_pw, None)
    add_command(exec, vault1_acct3_tag, vault1_key, vault1_acct3_un, vault1_acct3_pw, None)

    switch_vault_command(exec, vault2_name, vault2_key)
    add_command(exec, vault2_acct1_tag, vault2_key, vault2_acct1_un, vault2_acct1_pw, None)
    add_command(exec, vault2_acct2_tag, vault2_key, vault2_acct2_un, vault2_acct2_pw, None)
    add_command(exec, vault2_acct3_tag, vault2_key, vault2_acct3_un, vault2_acct3_pw, None)

    switch_vault_command(exec, vault3_name, vault3_key)
    add_command(exec, vault3_acct1_tag, vault3_key, vault3_acct1_un, vault3_acct1_pw, None)
    add_command(exec, vault3_acct2_tag, vault3_key, vault3_acct2_un, vault3_acct2_pw, None)
    add_command(exec, vault3_acct3_tag, vault3_key, vault3_acct3_un, vault3_acct3_pw, None)

    test_suite.assert_equals(build_console_output(vault3_name, vault1_name, vault2_name), list_command(exec))
    test_suite.assert_equals(build_console_output(vault3_acct1_tag, vault3_acct2_tag, vault3_acct3_tag), list_command(exec, vault3_key))
    #print(list_command(exec, vault3_key, info=True)) # do not test this command until output format is decided upon

    update_vault_command(exec, vault3_key, vault3_newkey)
    switch_vault_command(exec, vault2_name, vault2_key)
    switch_vault_command(exec, vault3_name, vault3_key) # invalid vault key
    test_suite.assert_equals(build_console_output(vault2_acct1_tag, vault2_acct2_tag, vault2_acct3_tag), list_command(exec, vault2_key))
    switch_vault_command(exec, vault3_name, vault3_newkey)
    test_suite.assert_equals(build_console_output(vault3_acct1_tag, vault3_acct2_tag, vault3_acct3_tag), list_command(exec, vault3_newkey))

    delete_vault_command(exec, vault3_name, vault3_newkey) # cannot delete active vault
    test_suite.assert_equals(build_console_output(vault3_name, vault1_name, vault2_name), list_command(exec))
    delete_vault_command(exec, vault2_name, vault2_key)
    test_suite.assert_equals(build_console_output(vault3_name, vault1_name), list_command(exec))

    add_vault_command(exec, vault1_name, vault1_key) # vault already exists
    switch_vault_command(exec, vault1_name, vault1_key)
    test_suite.assert_equals(build_console_output(vault1_acct1_tag, vault1_acct2_tag, vault1_acct3_tag), list_command(exec, vault1_key))

    test_suite.finish()

    clean_dir()

def add_vault_command(exec, new_vault_name, new_vault_key):
    return exec_cmd(construct_cmd(
        exec,
        CommandLineOptions.VAULT_OPTION,
        'add',
        CommandLineOptions.NAME_OPTION,
        new_vault_name,
        CommandLineOptions.KEY_OPTION,
        new_vault_key))

def update_vault_command(exec, old_key, new_key):
    return exec_cmd(construct_cmd(
        exec,
        CommandLineOptions.VAULT_OPTION,
        'update',
        CommandLineOptions.KEY_OPTION,
        old_key,
        CommandLineOptions.NEWKEY_OPTION,
        new_key))

def switch_vault_command(exec, vault_name, vault_key):
    return exec_cmd(construct_cmd(
        exec,
        CommandLineOptions.VAULT_OPTION,
        'switch',
        CommandLineOptions.NAME_OPTION,
        vault_name,
        CommandLineOptions.KEY_OPTION,
        vault_key))

def delete_vault_command(exec, vault_name, vault_key):
    return exec_cmd(construct_cmd(
        exec,
        CommandLineOptions.VAULT_OPTION,
        'delete',
        CommandLineOptions.NAME_OPTION,
        vault_name,
        CommandLineOptions.KEY_OPTION,
        vault_key))

def list_command(exec, vault_key=None, info=False):
    if vault_key and info:
        cmd = construct_cmd(
            exec,
            CommandLineOptions.VAULT_OPTION,
            'list',
            CommandLineOptions.KEY_OPTION,
            vault_key,
            CommandLineOptions.INFO_OPTION)
    elif vault_key:
        cmd = construct_cmd(
            exec,
            CommandLineOptions.VAULT_OPTION,
            'list',
            CommandLineOptions.KEY_OPTION,
            vault_key)
    else:
        cmd = construct_cmd(
            exec,
            CommandLineOptions.VAULT_OPTION,
            'list')
    return exec_cmd(cmd)

def test_print():
    # tests print commands
    clean_dir()

    exec = './pml'

    acct_tag = 'acct123'
    vault_key = 'key123'
    username = 'username123'
    password = 'password123'
    note = 'note123'

    test_suite = TestSuite('test_print')

    add_command(exec, acct_tag, vault_key, username, password, None)
    update_command(exec, acct_tag, vault_key, CommandLineOptions.NOTE_OPTION, note)

    test_suite.assert_equals(build_console_output('un=' + username, 'pw=' + password, 'note=' + note), print_command(exec, acct_tag, vault_key))
    test_suite.assert_equals(build_console_output(username), print_command(exec, acct_tag, vault_key, CommandLineOptions.USERNAME_OPTION))
    test_suite.assert_equals(build_console_output(password), print_command(exec, acct_tag, vault_key, CommandLineOptions.PASSWORD_OPTION))
    test_suite.assert_equals(build_console_output(note), print_command(exec, acct_tag, vault_key, CommandLineOptions.NOTE_OPTION))

    test_suite.finish()

    clean_dir()

def print_command(exec, account_name, vault_key, option=None):
    if option is None:
        cmd = construct_cmd(
            exec,
            CommandLineOptions.PRINT_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key)
    else:
        cmd = construct_cmd(
            exec,
            CommandLineOptions.PRINT_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key,
            option)
    return exec_cmd(cmd)

def get_clipboard_data():
    p = subprocess.Popen(['xclip','-selection', 'clipboard', '-o'], stdout=subprocess.PIPE)
    retcode = p.wait()
    data = p.stdout.read()
    return data

def test_clip():
    # tests clip commands
    clean_dir()

    exec = './pml'

    acct_tag = 'acct123'
    vault_key = 'key123'
    username = 'username123'
    password = 'password123'

    test_suite = TestSuite('test_clip')

    add_command(exec, acct_tag, vault_key, username, password, None)

    # TODO: Fix clip_command (hangs indefinitely)
    clip_command(exec, acct_tag, vault_key, CommandLineOptions.USERNAME_OPTION)
    #print(get_clipboard_data()) # assertion here
    test_suite.assert_equals(build_console_output(username), get_clipboard_data())

    # TODO: Fix clip_command (hangs indefinitely)
    clip_command(exec, acct_tag, vault_key, CommandLineOptions.PASSWORD_OPTION)
    #print(get_clipboard_data()) # assertion here
    test_suite.assert_equals(build_console_output(password), get_clipboard_data())

    test_suite.finish()

    clean_dir()

def clip_command(exec, account_name, vault_key, option):
    return exec_cmd(construct_cmd(
            exec,
            CommandLineOptions.CLIP_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key,
            option))

def test_update():
    # tests update commands
    clean_dir()

    exec = './pml'

    vault_name, vault_key = 'vault1', 'key1'

    acct1_tag, acct1_un, acct1_pw, acct1_note = 'acct1-1', 'un1-1', 'pw1-1', 'note1'
    acct1_un_new, acct1_pw_new, acct1_note_new = 'un-new', 'pw-new', 'note-new'
    acct2_tag, acct2_un, acct2_pw = 'acct1-2', 'un1-2', 'pw1-2'

    test_suite = TestSuite('test_update')

    add_command(exec, acct1_tag, vault_key, acct1_un, acct1_pw, None)
    update_command(exec, acct1_tag, vault_key, CommandLineOptions.NOTE_OPTION, acct1_note)

    test_suite.assert_equals(build_console_output(acct1_un), print_command(exec, acct1_tag, vault_key, CommandLineOptions.USERNAME_OPTION))
    test_suite.assert_equals(build_console_output(acct1_pw), print_command(exec, acct1_tag, vault_key, CommandLineOptions.PASSWORD_OPTION))
    test_suite.assert_equals(build_console_output(acct1_note), print_command(exec, acct1_tag, vault_key, CommandLineOptions.NOTE_OPTION))

    update_command(exec, acct1_tag, vault_key, CommandLineOptions.USERNAME_OPTION, acct1_un_new)
    update_command(exec, acct1_tag, vault_key, CommandLineOptions.PASSWORD_OPTION, acct1_pw_new)
    update_command(exec, acct1_tag, vault_key, CommandLineOptions.NOTE_OPTION, acct1_note_new)

    test_suite.assert_equals(build_console_output(acct1_un_new), print_command(exec, acct1_tag, vault_key, CommandLineOptions.USERNAME_OPTION))
    test_suite.assert_equals(build_console_output(acct1_pw_new), print_command(exec, acct1_tag, vault_key, CommandLineOptions.PASSWORD_OPTION))
    test_suite.assert_equals(build_console_output(acct1_note_new), print_command(exec, acct1_tag, vault_key, CommandLineOptions.NOTE_OPTION))

    add_command(exec, acct2_tag, vault_key, acct2_un, acct2_pw, None)
    test_suite.assert_equals(build_console_output(acct1_tag, acct2_tag), list_command(exec, vault_key))
    update_command(exec, acct1_tag, vault_key, CommandLineOptions.DELETE_OPTION)
    test_suite.assert_equals(build_console_output(acct2_tag), list_command(exec, vault_key))

    test_suite.finish()

    clean_dir()

def update_command(exec, account_name, vault_key, option, arg=None):
    if arg is None:
        cmd = construct_cmd(
            exec,
            CommandLineOptions.UPDATE_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key,
            option)
    else:
        cmd = construct_cmd(
            exec,
            CommandLineOptions.UPDATE_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key,
            option,
            arg)
    return exec_cmd(cmd)

def test_add():
    # tests add commands
    clean_dir()

    exec = './pml'

    vault_name, vault_key = 'vault1', 'key1'

    acct1_tag, acct1_un, acct1_pw, acct1_note = 'acct1-1', 'un1-1', 'pw1-1', 'note1'
    acct2_tag, acct2_un, acct2_pw = 'acct1-2', 'un1-2', 'pw1-2'

    test_suite = TestSuite('test_add')

    add_command(exec, acct1_tag, vault_key, acct1_un, acct1_pw, None)
    update_command(exec, acct1_tag, vault_key, CommandLineOptions.NOTE_OPTION, acct1_note)

    test_suite.assert_equals(build_console_output(acct1_un), print_command(exec, acct1_tag, vault_key, CommandLineOptions.USERNAME_OPTION))
    test_suite.assert_equals(build_console_output(acct1_pw), print_command(exec, acct1_tag, vault_key, CommandLineOptions.PASSWORD_OPTION))
    test_suite.assert_equals(build_console_output(acct1_note), print_command(exec, acct1_tag, vault_key, CommandLineOptions.NOTE_OPTION))

    add_command(exec, acct2_tag, vault_key, acct2_un, acct2_pw, None)
    test_suite.assert_equals(build_console_output(acct1_tag, acct2_tag), list_command(exec, vault_key))

    test_suite.assert_equals(build_console_output(acct2_un), print_command(exec, acct2_tag, vault_key, CommandLineOptions.USERNAME_OPTION))
    test_suite.assert_equals(build_console_output(acct2_pw), print_command(exec, acct2_tag, vault_key, CommandLineOptions.PASSWORD_OPTION))

    add_command(exec, acct1_tag, vault_key, acct1_un, acct1_pw, None) # account tag already exists error
    test_suite.assert_equals(build_console_output(acct1_note), print_command(exec, acct1_tag, vault_key, CommandLineOptions.NOTE_OPTION)) # acct1 note should be non-empty

    test_suite.finish()

    clean_dir()

def add_command(exec, account_name, vault_key, un=None, pw=None, file_path=None):
    if file_path is not None:
        cmd = construct_cmd(
            exec,
            CommandLineOptions.ADD_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key,
            CommandLineOptions.FILE_OPTION,
            file_path)
    elif un is not None and pw is not None:
        cmd = construct_cmd(
            exec,
            CommandLineOptions.ADD_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key,
            CommandLineOptions.USERNAME_OPTION,
            un,
            CommandLineOptions.PASSWORD_OPTION,
            pw)
    else:
        cmd = construct_cmd(
            exec,
            CommandLineOptions.ADD_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key)
    return exec_cmd(cmd)

if __name__ == "__main__":
    test_vault()
    test_print()
    #test_clip()
    test_update()
    test_add()
