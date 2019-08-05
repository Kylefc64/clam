# -*- coding: UTF-8 -*-
import sys
import string
import random
import subprocess
import os
from pathlib import Path

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
            print("Test " + str(self.total) + " failed. Expected: " + str(expected) + " Actual: " + str(actual))
    
    def finish(self):
        print("Test suite [" + self.test_name + "] finished. " + str(self.passed) + '/' + str(self.total) + " tests passed.")

class Account:
    def __init__(self, tag, username, password, note):
        self.tag = tag
        self.username = username
        self.password = password
        self.note = note

"""
    Returns the name of the program.
"""
def program_name():
    return 'clam'

"""
    Returns the location where program data is stored.
"""
def program_data_dir():
    return str(Path.home()) + '/.' + program_name() + '/'

"""
    Given the expected lines, builds a string containing all of those lines separated by newlines.
"""
def build_console_output(*lines):
    return ''.join([line + '\n' for line in lines]).rstrip('\n')

## Executes the given command and returns the output void of trailing whitespace.
def exec_cmd(cmd):
    # issues with python subprocess module cause deadlock when write size exceeds ~65,000 characters
    # a stackoverflow post and python 2 documentation suggest using communication() instead will prevent the issue, but that is not nenessarily true
    # https://stackoverflow.com/questions/14433712/python-popen-stdout-read-hang
    # So far the only working solution is to direct subprocess stdout into a file instead of PIPE, as detailed in the post below
    # https://thraxil.org/users/anders/posts/2008/03/13/Subprocess-Hanging-PIPE-is-your-enemy/

    # return subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE).stdout.read().decode('UTF-8').rstrip('\t\n ')
    tempfile = open('./test/temp', 'w+')
    process = subprocess.Popen(cmd, shell=True, stdout=tempfile)
    process.wait()
    tempfile.seek(0)
    exec_output = tempfile.read().rstrip('\t\n ')
    tempfile.close()
    return exec_output

def construct_cmd(*args):
    return ''.join([arg + ' ' for arg in args]).rstrip(' ')

def clean_dir():
    exec_cmd(construct_cmd('rm', '-rf', program_data_dir()))
    os.remove('./test/temp')

def test_vault(exec):
    # tests vault commands
    clean_dir()

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

def test_print(exec):
    # tests print commands
    clean_dir()

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
    data = p.stdout.read().decode('UTF-8').rstrip('\n')
    return data

def test_clip(exec):
    # tests clip commands
    clean_dir()

    acct_tag = 'acct123'
    vault_key = 'key123'
    username = 'username123'
    password = 'password123'

    test_suite = TestSuite('test_clip')

    add_command(exec, acct_tag, vault_key, username, password, None)

    clip_command(exec, acct_tag, vault_key, CommandLineOptions.USERNAME_OPTION)
    # print(get_clipboard_data()) # assertion here
    test_suite.assert_equals(build_console_output(username), get_clipboard_data())
    clip_command(exec, acct_tag, vault_key, CommandLineOptions.PASSWORD_OPTION)
    # print(get_clipboard_data()) # assertion here
    test_suite.assert_equals(build_console_output(password), get_clipboard_data())

    test_suite.finish()

    clean_dir()

def clip_command(exec, account_name, vault_key, option):
    cmd = construct_cmd(
            exec,
            CommandLineOptions.CLIP_OPTION,
            account_name,
            CommandLineOptions.KEY_OPTION,
            vault_key,
            option)
    return exec_cmd(cmd)

def test_update(exec):
    # tests update commands
    clean_dir()

    vault_name, vault_key = 'vault1', 'key1'

    acct1_tag, acct1_un, acct1_pw, acct1_note = 'acct1-1', 'un1-1', 'pw1-1', 'note1'
    acct1_un_new, acct1_pw_new, acct1_note_new = 'un-new1', 'pw-new1', 'note-new1'
    acct2_tag, acct2_un, acct2_pw, acct2_note = 'acct1-2', 'un1-2', 'pw1-2', 'note2'
    acct2_un_new, acct2_pw_new, acct2_note_new = 'un-new2', 'pw-new2', 'line1\nline2\nline3\nline4'
    
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
    update_command(exec, acct2_tag, vault_key, CommandLineOptions.NOTE_OPTION, acct2_note)
    test_suite.assert_equals(build_console_output(acct1_tag, acct2_tag), list_command(exec, vault_key))
    update_command(exec, acct1_tag, vault_key, CommandLineOptions.DELETE_OPTION)
    test_suite.assert_equals(build_console_output(acct2_tag), list_command(exec, vault_key))

    # Write account 2 data to file:
    acct2_input_file_text = acct2_un_new + '\n' + acct2_pw_new + '\n' + acct2_note_new
    acct2_file_path = program_data_dir() + 'acct2'
    acct2_file = open(acct2_file_path, "w")
    acct2_file.write(acct2_input_file_text)
    acct2_file.close()

    # File update command that should fail, and therefore not modify acct2 data:
    update_command(exec, acct2_tag, vault_key, CommandLineOptions.FILE_OPTION, program_data_dir() + 'acct212123')
    test_suite.assert_equals(build_console_output(acct2_un), print_command(exec, acct2_tag, vault_key, CommandLineOptions.USERNAME_OPTION))
    test_suite.assert_equals(build_console_output(acct2_pw), print_command(exec, acct2_tag, vault_key, CommandLineOptions.PASSWORD_OPTION))
    test_suite.assert_equals(build_console_output(acct2_note), print_command(exec, acct2_tag, vault_key, CommandLineOptions.NOTE_OPTION))

    # File update command that should pass:
    update_command(exec, acct2_tag, vault_key, CommandLineOptions.FILE_OPTION, acct2_file_path)
    test_suite.assert_equals(build_console_output(acct2_un_new), print_command(exec, acct2_tag, vault_key, CommandLineOptions.USERNAME_OPTION))
    test_suite.assert_equals(build_console_output(acct2_pw_new), print_command(exec, acct2_tag, vault_key, CommandLineOptions.PASSWORD_OPTION))
    test_suite.assert_equals(build_console_output(acct2_note_new), print_command(exec, acct2_tag, vault_key, CommandLineOptions.NOTE_OPTION))

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

def test_add(exec):
    # tests add commands
    clean_dir()

    vault_name, vault_key = 'vault1', 'key1'

    acct1_tag, acct1_un, acct1_pw, acct1_note = 'acct1-1', 'un1-1', 'pw1-1', 'note1'
    acct2_tag, acct2_un, acct2_pw = 'acct1-2', 'un1-2', 'pw1-2'
    acct3_tag, acct3_un, acct3_pw, acct3_note = 'acct1-3', 'un1-3', 'pw1-3', 'line1\nline2\nline3\nline4'

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

    # Write account 3 data to file:
    acct3_input_file_text = acct3_un + '\n' + acct3_pw + '\n' + acct3_note
    acct3_file_path = program_data_dir() + 'acct3'
    acct3_file = open(acct3_file_path, "w")
    acct3_file.write(acct3_input_file_text)
    acct3_file.close()

    # File add command that should pass:
    add_command(exec, acct3_tag, vault_key, None, None, acct3_file_path)
    test_suite.assert_equals(build_console_output(acct3_un), print_command(exec, acct3_tag, vault_key, CommandLineOptions.USERNAME_OPTION))
    test_suite.assert_equals(build_console_output(acct3_pw), print_command(exec, acct3_tag, vault_key, CommandLineOptions.PASSWORD_OPTION))
    test_suite.assert_equals(build_console_output(acct3_note), print_command(exec, acct3_tag, vault_key, CommandLineOptions.NOTE_OPTION))

    # File add command that should fail, and therefore not modify acct3 data:
    add_command(exec, acct3_tag, vault_key, None, None, str(Path.home()) + 'acct312123')
    test_suite.assert_equals(build_console_output(acct3_un), print_command(exec, acct3_tag, vault_key, CommandLineOptions.USERNAME_OPTION))
    test_suite.assert_equals(build_console_output(acct3_pw), print_command(exec, acct3_tag, vault_key, CommandLineOptions.PASSWORD_OPTION))
    test_suite.assert_equals(build_console_output(acct3_note), print_command(exec, acct3_tag, vault_key, CommandLineOptions.NOTE_OPTION))

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

def gen_rand_str(n=random.randint(1, 1024)):
    return ''.join(random.SystemRandom().choice(string.ascii_uppercase + string.digits) for _ in range(n))

def get_vault_filepath(vault_name):
    return program_data_dir() + 'vaults/' + vault_name

def read_raw_data(file_path):
    file = open(file_path, "rb")
    data = file.read()
    file.close()
    return data

"""
    Tests the cryptographic integrity of the application.
"""
def test_crypto(exec):

    clean_dir()

    str1, str2, str3 = 'kylecripps', 'apple', 'helloworld'

    ascii_string = 'fhahffhsappledafkjjsfhasdkylecrippsfSADFASdfSDFDsafhsdfhashjdfhelloworld'
    file = open('/home/kyle/test', "w")
    file.write(ascii_string)
    file.close()

    file = open('/home/kyle/test', "rb")
    raw_string = file.read()
    file.close()

    test_suite_pre = TestSuite('test_crypto_pre')
    test_suite_pre.assert_equals(True, str1.encode() in raw_string)
    test_suite_pre.assert_equals(True, str2.encode() in raw_string)
    test_suite_pre.assert_equals(True, str2.encode() in raw_string)
    test_suite_pre.finish()

    clean_dir()

    # For X iterations:
        # Generate a random value for a vault key and name
        # Execute an add vault command to create a vault using that key and name
        # Generate random values for an account tag, username, password, and note
        # Create an empty set of raw vault data
        # For Y iterations:
            # Execute an add command to add these acct details to the vault
            # Execute a print command(s) and verify that these account details were correctly encrypted and decrypted
            # Parse the vault file to verify that it does not contain a substring that is equal to the acct tag, username, password, or note
            # Store the raw vault data in a set
            # Iterate through the set of raw vault data and assert that no entry in the set is equal to another

    clean_dir()

    test_suite = TestSuite('test_crypto')

    # Number of random datasets to re-encrypt & number of times to re-encrypt each dataset:
    num_datasets, num_encryptions = 20, 100

    total_iterations = num_datasets * num_encryptions

    for i in range(num_datasets):

        sys.stdout.write("\033[K") # clear line
        sys.stdout.write('Running test_crypto: ' + str(100 * (i*num_encryptions / total_iterations)) + '% complete...\r')
        sys.stdout.flush()

        # Create vault using randomly generated vault name and key:
        vault_key = gen_rand_str()
        vault_name = gen_rand_str(255)
        add_vault_command(exec, vault_name, vault_key)
        switch_vault_command(exec, vault_name, vault_key)

        # Generate random account data:
        acct_tag = gen_rand_str()
        acct_tag_bin = acct_tag.encode()
        acct_username = gen_rand_str()
        acct_username_bin = acct_username.encode()
        acct_password = gen_rand_str()
        acct_password_bin = acct_password.encode()
        acct_note = gen_rand_str()
        acct_note_bin = acct_note.encode()

        encrypted_vault_data_set = set() # set of raw encrypted vault data for this plaintext dataset

        for j in range(num_encryptions):
            add_command(exec, acct_tag, vault_key, acct_username, acct_password, None)
            update_command(exec, acct_tag, vault_key, CommandLineOptions.NOTE_OPTION, acct_note)
            test_suite.assert_equals(build_console_output('un=' + acct_username, 'pw=' + acct_password, 'note=' + acct_note).encode(),
                                     print_command(exec, acct_tag, vault_key).encode())

            encrypted_vault_data = read_raw_data(get_vault_filepath(vault_name))

            # Assert that the encrypted file contains no substrings equal to any of the raw plaintext input data:
            test_suite.assert_equals(True, acct_tag_bin not in encrypted_vault_data
                                       and acct_username_bin not in encrypted_vault_data
                                       and acct_password_bin not in encrypted_vault_data
                                       and acct_note_bin not in encrypted_vault_data)

            # Assert that the raw encrypted file contents has not been seen before for the same set of account data:
            test_suite.assert_equals(True, encrypted_vault_data not in encrypted_vault_data_set)
            encrypted_vault_data_set.add(encrypted_vault_data)

            # Remove the account dataset from the vault so that it can be re-added and re-encrypted next iteration:
            update_command(exec, acct_tag, vault_key, CommandLineOptions.DELETE_OPTION)


    test_suite.finish()

    clean_dir()


if __name__ == "__main__":
    exec = './bin/' + program_name()
    test_vault(exec)
    test_print(exec)
    test_clip(exec)
    test_update(exec)
    test_add(exec)
    test_crypto(exec)
