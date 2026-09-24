# ft_ping

42 project: recode the ping command

## inetutils-2.0

- The ping implementation from inetutils-2.0 is used as a reference.
- inetutils-2.0 can be run in a docker using the following commands:

  ```sh
  docker build . -t ping2
  docker run --rm ping2 something.com
  ```

## ft_ping Options

- Available options: -v -?.

# resources

- [inetutils](https://www.gnu.org/software/inetutils/)
- [ping](https://www.gnu.org/software/inetutils/manual/html_node/ping-invocation.html)
