<?php
error_reporting(E_ALL);
ini_set('display_errors', 'on');

if (php_sapi_name() != 'cli'){
	header('Content-Type: text/plain');
}

defined('PHP_EOL') or define('PHP_EOL', "\n");

if(!extension_loaded('rtrack')) {
	echo "Warn: rtrack not loaded yet. Try to use dl() to load: " . 'rtrack.' . PHP_SHLIB_SUFFIX . PHP_EOL;
	dl('rtrack.' . PHP_SHLIB_SUFFIX);
}

$module = 'rtrack';
$functions = get_extension_funcs($module);
echo "Functions available in the test extension:" . PHP_EOL;
foreach($functions as $func) {
    echo $func . PHP_EOL;
}

echo PHP_EOL;


if (extension_loaded($module)) {
	echo confirm_rtrack_compiled($module);
} else {
	echo "Module rtrack is not compiled into PHP." . PHP_EOL;
	exit(1);
}

function run($cmd, &$return_var=null){
	echo "> " . $cmd . PHP_EOL;
	return system($cmd, $return_var);
}

file_put_contents("test-include.php", "<?php echo 'hello world!' . PHP_EOL;");
assert("file_exists('test-include.php')");

assert("!file_exists('require.log')");

ini_set('rtrack.log_file', 'require.log');

require('test-include.php');

run("ls -al *.log");
run("cat require.log");

assert("file_exists('require.log')");




run("ls -al *.log");
run("cat require.log");

assert("strpos(file_get_contents('require.log'), 'test-include.php')");


function hook_rtrack($fileName){
	echo "HOOK RTRACK: " . $fileName . PHP_EOL;
}

echo "Hook function: " . rtrack_get_hook_func() . PHP_EOL;
assert("rtrack_get_hook_func() === ''");

rtrack_set_hook_func('hook_rtrack');

echo "Hook function: " . rtrack_get_hook_func() . PHP_EOL;
assert("rtrack_get_hook_func() === 'hook_rtrack'");

require('test-include.php');

run("ls -al *.log");
run("cat require.log");

echo "Total required/included files count: " . rtrack_count() . PHP_EOL;
